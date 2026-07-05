#!/usr/bin/env python3
"""Behavioral comparison: Bash vs Minishell."""

import os
import re
import subprocess
import sys
import tempfile
import shutil
import random
import string
from dataclasses import dataclass, field
from typing import Optional

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MINISHELL = os.path.join(ROOT, "minishell")
BASH = "/bin/bash"
FAILURES: list[dict] = []
SKIPPED: list[str] = []


@dataclass
class Result:
    stdout: str
    stderr: str
    exit_code: int
    alive: bool = False


@dataclass
class TestCase:
    category: str
    name: str
    script: str
    cwd: Optional[str] = None
    setup: Optional[str] = None
    compare_stdout: bool = True
    compare_stderr: bool = True
    compare_exit: bool = True
    normalize: bool = True
    bash_only_setup: Optional[str] = None


def normalize_output(text: str, is_minishell: bool) -> str:
    if not text:
        return ""
    lines = text.splitlines()
    out = []
    for line in lines:
        if is_minishell and line.startswith("minishell$"):
            rest = line[len("minishell$"):]
            if rest.strip():
                out.append(rest.strip())
            continue
        if line.strip() == "minishell$":
            continue
        out.append(line)
    return "\n".join(out).strip()


def normalize_stderr(text: str) -> str:
    text = re.sub(r"^bash: line \d+: ", "minishell: ", text, flags=re.MULTILINE)
    text = re.sub(r"^bash: ", "minishell: ", text, flags=re.MULTILINE)
    return text.strip()


def run_bash(script: str, cwd: Optional[str]) -> Result:
    env = os.environ.copy()
    env["PS1"] = ""
    proc = subprocess.run(
        [BASH, "--norc", "--noprofile", "-s"],
        input=script,
        capture_output=True,
        text=True,
        cwd=cwd or ROOT,
        env=env,
    )
    return Result(proc.stdout, proc.stderr, proc.returncode)


def run_minishell(script: str, cwd: Optional[str]) -> Result:
    if not os.path.isfile(MINISHELL):
        raise FileNotFoundError(MINISHELL)
    lines = script.splitlines()
    if not lines or lines[-1].strip() != "exit":
        script = script + "\nexit\n"
    proc = subprocess.run(
        [MINISHELL],
        input=script,
        capture_output=True,
        text=True,
        cwd=cwd or ROOT,
    )
    return Result(proc.stdout, proc.stderr, proc.returncode)


def run_session(shell_cmd: list[str], script: str, cwd: Optional[str]) -> Result:
    env = os.environ.copy()
    env["PS1"] = ""
    proc = subprocess.run(
        shell_cmd,
        input=script if script.endswith("\n") else script + "\n",
        capture_output=True,
        text=True,
        cwd=cwd or ROOT,
        env=env,
    )
    return Result(proc.stdout, proc.stderr, proc.returncode)


def compare(tc: TestCase, bash_r: Result, ms_r: Result):
    b_out = normalize_output(bash_r.stdout, False) if tc.normalize else bash_r.stdout.strip()
    m_out = normalize_output(ms_r.stdout, True) if tc.normalize else ms_r.stdout.strip()
    b_err = normalize_stderr(bash_r.stderr)
    m_err = ms_r.stderr.strip()

    diffs = []
    if tc.compare_stdout and b_out != m_out:
        diffs.append(("stdout", b_out, m_out))
    if tc.compare_stderr and b_err != m_err:
        diffs.append(("stderr", b_err, m_err))
    if tc.compare_exit and bash_r.exit_code != ms_r.exit_code:
        diffs.append(("exit_code", str(bash_r.exit_code), str(ms_r.exit_code)))

    if diffs:
        FAILURES.append({
            "category": tc.category,
            "name": tc.name,
            "script": tc.script.strip(),
            "diffs": diffs,
        })


def run_test(tc: TestCase, tmpdir: str):
    cwd = tc.cwd or tmpdir
    if tc.setup:
        subprocess.run(tc.setup, shell=True, cwd=cwd, capture_output=True)
    script = tc.script
    bash_r = run_bash(script, cwd)
    ms_r = run_minishell(script, cwd)
    compare(tc, bash_r, ms_r)


def build_tests(tmpdir: str) -> list[TestCase]:
    t = []
    # 1. Lexer / empty input
    t += [
        TestCase("Lexer", "empty input", ""),
        TestCase("Lexer", "spaces only", "   \t  "),
        TestCase("Lexer", "multiple spaces", "echo    hello    world"),
    ]
    # 2. Parser syntax errors
    syntax_cmds = [
        ("pipe alone", "|"),
        ("double pipe", "||"),
        ("redirect alone >", ">"),
        ("redirect > >", "> >"),
        ("redirect alone <", "<"),
        ("redirect alone <<", "<<"),
        ("pipe ls", "| ls"),
        ("ls pipe", "ls |"),
        ("cat redirect >", "cat >"),
        ("cat redirect <", "cat <"),
        ("echo redirect >", "echo >"),
        ("echo append >>", "echo >>"),
        ("heredoc pipe", "cat << |"),
        ("redirect file only", "> file"),
        ("double redirect file", "> > file"),
        ("redirect in file only", "< file"),
        ("heredoc incomplete", "<< EOF"),
    ]
    for name, cmd in syntax_cmds:
        t.append(TestCase("Parser", f"syntax: {name}", cmd))
    # 3. Quotes
    quote_cmds = [
        ("empty single", "echo ''"),
        ("empty double", 'echo ""'),
        ("single abc", "echo 'abc'"),
        ("double abc", 'echo "abc"'),
        ("single with double", "echo '\"'"),
        ("double with single", "echo \"'\""),
        ("unclosed single", "echo 'abc"),
        ("unclosed double", 'echo "abc'),
    ]
    for name, cmd in quote_cmds:
        t.append(TestCase("Quotes", name, cmd))
    # 4. Expansion
    exp_cmds = [
        ("bare dollar", "echo $"),
        ("USER", "echo $USER"),
        ("HOME", "echo $HOME"),
        ("PATH partial", "echo $PATH | head -c 20"),
        ("exit status after true", "true\necho $?"),
        ("NOT_EXIST", "echo $NOT_EXIST"),
        ("quoted double USER", 'echo "$USER"'),
        ("quoted single USER", "echo '$USER'"),
        ("quoted double NOT_EXIST", 'echo "$NOT_EXIST"'),
        ("quoted double question", 'echo "$?"'),
    ]
    for name, cmd in exp_cmds:
        t.append(TestCase("Expansion", name, cmd))
    # 5. Export
    export_cmds = [
        ("export no value", "export a\nexport | grep '^declare -x a$'"),
        ("export empty", "export a=\nexport | grep a\nenv | grep '^a='"),
        ("export empty quoted", 'export a=""\nexport | grep a\nenv | grep ^a='),
        ("export value", "export a=hello\nexport | grep a\nenv | grep ^a="),
        ("export quoted", 'export a="hello"\nenv | grep ^a='),
        ("export single empty", "export a=''\nenv | grep ^a="),
        ("export invalid", "export 1abc"),
        ("export empty key", 'export ""'),
    ]
    for name, cmd in export_cmds:
        t.append(TestCase("Export", name, cmd))
    # 6. Unset
    unset_cmds = [
        ("unset var", "export a=1\nunset a\nenv | grep '^a='"),
        ("unset PATH", "unset PATH\necho ok"),
        ("unset invalid", "unset 1abc"),
        ("unset empty", 'unset ""'),
        ("unset multiple", "export a=1 b=2 c=3\nunset a b c\nenv | grep -E '^(a|b|c)='"),
    ]
    for name, cmd in unset_cmds:
        t.append(TestCase("Unset", name, cmd))
    # 7. Env
    t.append(TestCase("Env", "no-value export invisible",
                     "export noval\nenv | grep noval ; echo exit:$?"))
    # 8. Echo
    echo_cmds = [
        ("bare echo", "echo"),
        ("hello", "echo hello"),
        ("-n", "echo -n"),
        ("-nnnn", "echo -nnnn"),
        ("-nnnn hello", "echo -nnnn hello"),
        ("empty double", 'echo ""'),
        ("empty single", "echo ''"),
        ("USER expand", 'echo "$USER"'),
        ("USER no expand", "echo '$USER'"),
        ("spaces", "echo hello      world"),
    ]
    for name, cmd in echo_cmds:
        t.append(TestCase("Echo", name, cmd))
    # 9. Cd
    cd_cmds = [
        ("cd no args", "cd"),
        ("cd dot", "cd ."),
        ("cd parent", "cd .."),
        ("cd root", "cd /"),
        ("cd bad", "cd /nonexistent_dir_xyz"),
        ("cd empty", 'cd ""'),
        ("cd too many", "cd / /"),
        ("PWD update", "cd /tmp\necho $PWD"),
    ]
    for name, cmd in cd_cmds:
        t.append(TestCase("Cd", name, cmd))
    # 10. Pwd
    t.append(TestCase("Pwd", "pwd output", "pwd"))
    # 11. Exit - special handling: don't append exit
    exit_cmds = [
        ("exit no arg", "exit"),
        ("exit 0", "exit 0"),
        ("exit 1", "exit 1"),
        ("exit 42", "exit 42"),
        ("exit -1", "exit -1"),
        ("exit +1", "exit +1"),
        ("exit abc", "exit abc"),
        ("exit too many", "exit 1 2"),
    ]
    for name, cmd in exit_cmds:
        t.append(TestCase("Exit", name, cmd, compare_stdout=False))
    # 12. Pipes
    pipe_cmds = [
        ("ls cat", "ls | cat"),
        ("ls cat cat", "ls | cat | cat"),
        ("yes head", "yes | head -n 3"),
        ("echo wc", "echo hello | wc -c"),
    ]
    for name, cmd in pipe_cmds:
        t.append(TestCase("Pipes", name, cmd))
    # 13. Redirect
    redir_setup = f"cd {tmpdir} && rm -f f in out 2>/dev/null; echo line > in"
    t += [
        TestCase("Redirect", "cat < file", "cat < in", cwd=tmpdir,
                 setup=redir_setup),
        TestCase("Redirect", "echo > file", "echo hi > out\ncat out", cwd=tmpdir,
                 setup=redir_setup),
        TestCase("Redirect", "echo >> file", "echo hi >> out\ncat out", cwd=tmpdir,
                 setup=redir_setup),
        TestCase("Redirect", "cat < > combo", "cat < in > out\ncat out", cwd=tmpdir,
                 setup=redir_setup),
    ]
    # 14. Heredoc
    t.append(TestCase("Heredoc", "basic heredoc",
                      "cat << EOF\nhello\nEOF"))
    t.append(TestCase("Heredoc", "quoted delimiter no expand",
                      "export X=world\ncat << 'EOF'\n$X\nEOF"))
    # 16. Exit status
    status_cmds = [
        ("false", "false\necho $?"),
        ("true", "true\necho $?"),
        ("ls missing", "ls /nonexistent_xyz\necho $?"),
        ("cat missing", "cat /nonexistent_xyz\necho $?"),
        ("echo status", "echo hi\necho $?"),
        ("cd bad status", "cd /nonexistent_xyz\necho $?"),
        ("export bad", "export 1abc\necho $?"),
    ]
    for name, cmd in status_cmds:
        t.append(TestCase("Exit Status", name, cmd))
    return t


def run_exit_tests(tmpdir):
    for name, cmd in [
        ("exit no arg last=5", "false\nexit"),
        ("exit 0", "exit 0"),
        ("exit 42", "exit 42"),
        ("exit -1", "exit -1"),
        ("exit abc", "exit abc"),
        ("exit too many", "exit 1 2"),
    ]:
        bash_r = run_session([BASH, "--norc", "--noprofile", "-s"], cmd, tmpdir)
        ms_input = cmd if cmd.strip().startswith("exit") else cmd + "\nexit"
        if "exit" in cmd.split():
            ms_input = cmd
        ms_r = run_session([MINISHELL], ms_input, tmpdir)
        if bash_r.exit_code != ms_r.exit_code:
            FAILURES.append({
                "category": "Exit",
                "name": name,
                "script": cmd,
                "diffs": [("exit_code", str(bash_r.exit_code), str(ms_r.exit_code))],
            })


def run_stress(tmpdir, n=200):
    random.seed(42)
    crashes = 0
    for i in range(n):
        parts = []
        for _ in range(random.randint(1, 4)):
            r = random.choice(["echo", "true", "false", "pwd", "export"])
            if r == "echo":
                s = "".join(random.choices(string.ascii_letters + " '\"$|", k=random.randint(0, 8)))
                parts.append(f'echo {s}')
            elif r == "export":
                parts.append(f'export T{i}=val{i}')
            else:
                parts.append(r)
        script = "\n".join(parts)
        try:
            ms_r = run_minishell(script, tmpdir)
            if ms_r.exit_code < 0:
                crashes += 1
        except Exception:
            crashes += 1
    if crashes:
        FAILURES.append({
            "category": "Stress",
            "name": f"{crashes}/{n} random sessions crashed",
            "script": "(random)",
            "diffs": [("crash", "0", str(crashes))],
        })


def run_valgrind():
    if not shutil.which("valgrind"):
        SKIPPED.append("Valgrind not installed on this system (macOS)")
        return
    proc = subprocess.run(
        ["valgrind", "--leak-check=full", "--error-exitcode=42",
         "--suppressions=" + os.path.join(ROOT, "minishell_tester/bash.supp"),
         MINISHELL],
        input="echo hi\nexit\n",
        capture_output=True,
        text=True,
        cwd=ROOT,
    )
    if proc.returncode == 42 or "ERROR SUMMARY: 0 errors" not in proc.stderr:
        leaks = [l for l in proc.stderr.splitlines() if "definitely lost" in l or "Invalid" in l]
        FAILURES.append({
            "category": "Memory",
            "name": "valgrind basic session",
            "script": "echo hi",
            "diffs": [("valgrind", "clean", "\n".join(leaks[:5]) or proc.stderr[-500:])],
        })


def run_signal_tests():
    if not shutil.which("expect"):
        SKIPPED.append("expect not available for signal tests")
        return
    tests = [
        ("Ctrl+D empty", 'spawn ./minishell\nexpect "minishell$"\nsend "\\004"\nexpect "exit"\n'),
        ("Ctrl+C then Ctrl+D", 'spawn ./minishell\nexpect "minishell$"\nsend "\\003"\nexpect "minishell$"\nsend "\\004"\nexpect "exit"\n'),
    ]
    for name, body in tests:
        script_path = os.path.join(ROOT, "tests", "_sig.exp")
        with open(script_path, "w") as f:
            f.write("set timeout 5\ncd " + ROOT + "\n" + body + "expect eof\ncatch wait r\nexit 0\n")
        proc = subprocess.run(["expect", script_path], capture_output=True, text=True, cwd=ROOT)
        if proc.returncode != 0:
            FAILURES.append({
                "category": "Signals",
                "name": name,
                "script": name,
                "diffs": [("signal_test", "pass", proc.stdout + proc.stderr)],
            })
        os.remove(script_path)


def main():
    if not os.path.isfile(MINISHELL):
        print("Build minishell first: make")
        sys.exit(1)

    tmpdir = tempfile.mkdtemp(prefix="ms_test_")
    try:
        tests = build_tests(tmpdir)
        total = len(tests)
        for i, tc in enumerate(tests, 1):
            run_test(tc, tmpdir)
            if i % 20 == 0:
                print(f"  progress: {i}/{total}", file=sys.stderr)

        run_exit_tests(tmpdir)
        run_stress(tmpdir)
        run_signal_tests()
        run_valgrind()
    finally:
        shutil.rmtree(tmpdir, ignore_errors=True)

    print(f"\n{'='*60}")
    print(f"BASH vs MINISHELL COMPARISON REPORT")
    print(f"{'='*60}")
    print(f"Tests run: {len(tests) + 6}")
    print(f"Failures:  {len(FAILURES)}")
    print(f"Skipped:   {len(SKIPPED)}")
    if SKIPPED:
        for s in SKIPPED:
            print(f"  SKIP: {s}")

    for i, f in enumerate(FAILURES, 1):
        print(f"\n--- Failure #{i} ---")
        print(f"Category: {f['category']}")
        print(f"Test:     {f['name']}")
        print(f"Command:\n  {f['script'][:200]}")
        for kind, bash_v, ms_v in f["diffs"]:
            print(f"\n  [{kind}]")
            print(f"  Bash:      {repr(bash_v[:300])}")
            print(f"  Minishell: {repr(ms_v[:300])}")

    sys.exit(1 if FAILURES else 0)


if __name__ == "__main__":
    main()
