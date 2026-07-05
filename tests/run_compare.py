#!/usr/bin/env python3
"""Bash vs Minishell behavioral comparison — clean output."""

import os
import re
import subprocess
import sys
import tempfile
import shutil

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MS = os.path.join(ROOT, "minishell")
BASH = "/bin/bash"
FAILURES = []


def norm_ms_out(text: str) -> str:
    lines = []
    for line in text.splitlines():
        if line.startswith("minishell$"):
            rest = line[len("minishell$"):].strip()
            if rest:
                lines.append(rest)
            continue
        if line.strip() == "minishell$" or line.strip() == "exit":
            continue
        lines.append(line)
    return "\n".join(lines).strip()


def norm_bash_err(text: str) -> str:
    text = re.sub(r"^bash: line \d+: ", "", text, flags=re.MULTILINE)
    text = re.sub(r"^bash: ", "", text, flags=re.MULTILINE)
    return text.strip()


def norm_ms_err(text: str) -> str:
    text = re.sub(r"^minishell: ", "", text, flags=re.MULTILINE)
    return text.strip()


def run_bash(script: str, cwd: str) -> tuple[str, str, int]:
    p = subprocess.run(
        [BASH, "--norc", "--noprofile", "-s"],
        input=script, capture_output=True, text=True, cwd=cwd,
    )
    return p.stdout.strip(), p.stderr.strip(), p.returncode


def run_ms(script: str, cwd: str, append_exit: bool = True) -> tuple[str, str, int]:
    lines = [l for l in script.splitlines()]
    if append_exit and (not lines or lines[-1].strip() != "exit"):
        script = script.rstrip() + "\nexit\n"
    p = subprocess.run([MS], input=script, capture_output=True, text=True, cwd=cwd)
    return norm_ms_out(p.stdout), p.stderr.strip(), p.returncode


def check(cat, name, script, cwd=None, cmp_out=True, cmp_err=True, cmp_exit=True,
          append_exit=True, norm_err=True):
    cwd = cwd or ROOT
    bo, be, bc = run_bash(script, cwd)
    mo, me, mc = run_ms(script, cwd, append_exit=append_exit)
    diffs = []
    if cmp_out and bo != mo:
        diffs.append(("stdout", bo, mo))
    if cmp_err:
        be_n = norm_bash_err(be) if norm_err else be
        me_n = norm_ms_err(me) if norm_err else me
        if be_n != me_n:
            diffs.append(("stderr", be_n, me_n))
    if cmp_exit and bc != mc:
        diffs.append(("exit_code", str(bc), str(mc)))
    if diffs:
        FAILURES.append({"cat": cat, "name": name, "cmd": script.strip(), "diffs": diffs})


def main():
    if not os.path.isfile(MS):
        print("Önce: make")
        sys.exit(1)

    tmp = tempfile.mkdtemp(prefix="mstest_")
    try:
        subprocess.run(f"echo line > {tmp}/in", shell=True)
        open(f"{tmp}/out", "w").close()

        # 1 Lexer
        check("Lexer", "boş input", "")
        check("Lexer", "sadece boşluk", "   \t  ")
        check("Lexer", "çoklu boşluk", "echo    hello    world")

        # 2 Parser
        for cmd in ["|", "||", ">", "> >", "<", "<<", "| ls", "ls |",
                    "cat >", "cat <", "echo >", "echo >>", "cat << |",
                    "> file", "> > file", "< file", "<< EOF"]:
            check("Parser", f"syntax: {cmd!r}", cmd)

        # 3 Quotes
        for cmd in ["echo ''", 'echo ""', "echo 'abc'", 'echo "abc"',
                    "echo \"'\"", "echo '\"'", "echo 'abc", 'echo "abc']:
            check("Quotes", cmd, cmd, norm_err=False)

        # 4 Expansion
        for cmd in ["echo $", "echo $USER", "echo $NOT_EXIST", "echo $?",
                    'echo "$USER"', "echo '$USER'", 'echo "$NOT_EXIST"',
                    "false", "echo $?"]:
            check("Expansion", cmd.split("\n")[-1], cmd)

        # 5 Export
        check("Export", "export a (değersiz)",
              "export a\nexport | grep '^declare -x a$'")
        check("Export", "export a=",
              "export a=\nexport | grep 'declare -x a=\"\"'\nenv | grep '^a='")
        check("Export", 'export a=""',
              'export a=""\nexport | grep \'declare -x a=""\'\nenv | grep \'^a=\'')
        check("Export", "export a=hello",
              "export a=hello\nexport | grep 'declare -x a=\"hello\"'\nenv | grep '^a=hello'")
        check("Export", "export geçersiz",
              "export 1abc\necho $?")
        check("Export", "export sıralama",
              "export b=2\nexport a=1\nexport | grep 'declare -x [ab]='")

        # 6 Unset
        check("Unset", "unset a", "export a=1\nunset a\nenv | grep '^a=' ; echo rc:$?")
        check("Unset", "unset geçersiz", "unset 1abc\necho $?")
        check("Unset", "unset =", "unset =\necho $?")

        # 7 Env
        check("Env", "değersiz export env'de yok",
              "export noval\nenv | grep noval ; echo rc:$?")

        # 8 Echo
        check("Echo", "echo -n", "echo -n")
        check("Echo", "echo -nnnn", "echo -nnnn hi")
        check("Echo", "echo -nnnn hello", "echo -nnnn hello")
        check("Echo", "echo boşluk", "echo hello      world")

        # 9 Cd
        check("Cd", "cd yok", "cd /nonexistent_xyz_abc\necho $?")
        check("Cd", "cd ..", "cd ..\necho ok")
        check("Cd", "cd fazla arg", "cd / /\necho $?")

        # 10 Pwd
        check("Pwd", "pwd", "pwd")

        # 11 Exit
        check("Exit", "exit (arg yok, last=1)", "false\nexit",
              append_exit=False, cmp_out=False)
        check("Exit", "exit 42", "exit 42", append_exit=False, cmp_out=False)
        check("Exit", "exit abc", "exit abc", append_exit=False, cmp_out=False)
        check("Exit", "exit çok arg", "exit 1 2\necho $?", cmp_out=False)

        # 12 Pipes
        check("Pipes", "ls | cat", "ls | cat")
        check("Pipes", "ls | cat | cat", "ls | cat | cat")
        check("Pipes", "false | true → $?", "false | true\necho $?")
        check("Pipes", "true | false → $?", "true | false\necho $?")
        check("Pipes", "echo | wc", "echo hello | wc -c")

        # 13 Redirect
        check("Redirect", "cat < in", f"cat < {tmp}/in", cwd=tmp)
        check("Redirect", "echo > out", f"echo hi > {tmp}/out\ncat {tmp}/out", cwd=tmp)
        check("Redirect", "echo >> out", f"echo hi >> {tmp}/out\ncat {tmp}/out", cwd=tmp)

        # 14 Heredoc
        check("Heredoc", "temel", "cat << EOF\nhello\nEOF")
        check("Heredoc", "quoted delimiter",
              "export X=world\ncat << 'EOF'\n$X\nEOF")
        check("Heredoc", "unquoted expand",
              "export X=world\ncat << EOF\n$X\nEOF")

        # 16 Exit status
        check("Exit Status", "ls yok", "ls /nonexistent_xyz\necho $?")
        check("Exit Status", "export bad", "export 1abc\necho $?")

    finally:
        shutil.rmtree(tmp, ignore_errors=True)

    # Signals
    if shutil.which("expect"):
        for name, body in [
            ("Ctrl+D boş satır", 'expect "minishell$"\nsend "\\004"\nexpect "exit"'),
            ("Ctrl+C sonra Ctrl+D",
             'expect "minishell$"\nsend "\\003"\nexpect "minishell$"\nsend "\\004"\nexpect "exit"'),
        ]:
            exp = f"set timeout 5\ncd {ROOT}\nspawn ./minishell\n{body}\nexpect eof\n"
            p = subprocess.run(["expect", "-c", exp], capture_output=True, text=True, cwd=ROOT)
            if p.returncode != 0:
                FAILURES.append({"cat": "Signals", "name": name,
                                 "cmd": name, "diffs": [("signal", "pass", p.stderr[-200:])]})
    else:
        print("NOT: expect yok — sinyal testleri atlandı\n")

    # Memory
    if shutil.which("leaks"):
        p = subprocess.run(
            ["leaks", "--atExit", "--", MS],
            input="echo hi\nexit\n", capture_output=True, text=True, cwd=ROOT,
        )
        if "0 leaks" not in p.stderr and "0 total leaked bytes" not in p.stderr:
            FAILURES.append({"cat": "Memory", "name": "leaks",
                             "cmd": "echo hi", "diffs": [("leaks", "0 leaks", p.stderr[-300:])]})
    elif shutil.which("valgrind"):
        p = subprocess.run(
            ["valgrind", "--leak-check=full", "--error-exitcode=42", MS],
            input="echo hi\nexit\n", capture_output=True, text=True, cwd=ROOT,
        )
        if p.returncode != 0:
            FAILURES.append({"cat": "Memory", "name": "valgrind",
                             "cmd": "echo hi", "diffs": [("valgrind", "clean", "errors")]})

    # Report
    print("=" * 60)
    print(f"BASH vs MINISHELL — HATA RAPORU")
    print("=" * 60)
    print(f"Toplam uyumsuzluk: {len(FAILURES)}\n")

    if not FAILURES:
        print("Tüm testler geçti!")
        return

    for i, f in enumerate(FAILURES, 1):
        print(f"#{i} [{f['cat']}] {f['name']}")
        print(f"   Komut: {f['cmd'][:100]}")
        for kind, b, m in f["diffs"]:
            print(f"   {kind}:")
            print(f"     Bash:      {repr(b[:200])}")
            print(f"     Minishell: {repr(m[:200])}")
        print()

    sys.exit(1)


if __name__ == "__main__":
    main()
