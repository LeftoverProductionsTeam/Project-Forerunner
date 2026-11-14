#!/usr/bin/env python3
"""Reference client for the native Spark discovery contract; no live editing.

    python tools/spark_runtime.py --engine path/to/halo.exe --require runtime.discover

Checks --help before querying so an older runtime that ignores unknown options
is refused instead of accidentally starting a game.
"""
import argparse
import json
import os
from pathlib import Path
import subprocess
import sys

SCHEMA = "spark-forerunner.runtime-capabilities"


class RuntimeContractError(ValueError):
    pass


def discover(executable, required=(), version=1):
    if type(version) is not int or version != 1:
        raise RuntimeContractError("this reference client supports Spark contract version 1")
    executable = Path(executable).resolve()
    if not executable.is_file():
        raise RuntimeContractError(f"no engine executable: {executable}")
    environment = {k: v for k, v in os.environ.items() if not k.startswith("HALO_")}
    environment.update(HALO_UPDATE_AUTO="false", HALO_NET_ONLINE="false", HALO_CRASH_REPORTS="no")

    def invoke(arguments):
        result = subprocess.run([str(executable), *arguments], cwd=executable.parent, env=environment,
                                capture_output=True, text=True, timeout=15,
                                creationflags=subprocess.CREATE_NO_WINDOW if os.name == "nt" else 0)
        if result.returncode:
            raise RuntimeContractError(f"engine rejected discovery (exit {result.returncode}): "
                                       + result.stderr.strip()[:1024])
        if len(result.stdout) > 65536:
            raise RuntimeContractError("engine discovery response exceeds 64 KiB")
        return result.stdout

    if "--spark-capabilities" not in invoke(["--help"]):
        raise RuntimeContractError("this runtime does not advertise the Spark discovery contract")
    try:
        reply = json.loads(invoke(["--spark-capabilities", str(version)]))
    except json.JSONDecodeError as error:
        raise RuntimeContractError("engine discovery is not one valid JSON document") from error
    if (not isinstance(reply, dict) or reply.get("schema") != SCHEMA or
            type(reply.get("contract_version")) is not int or reply["contract_version"] != version):
        raise RuntimeContractError("engine returned an incompatible Spark discovery schema/version")
    operations = reply.get("operations")
    if not isinstance(operations, list) or any(not isinstance(op, str) for op in operations):
        raise RuntimeContractError("engine discovery has no valid operation list")
    missing = sorted(set(required) - set(operations))
    if missing:
        raise RuntimeContractError("unsupported required operation(s): " + ", ".join(missing))
    return reply


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--engine", required=True, type=Path)
    parser.add_argument("--require", action="append", default=[])
    parser.add_argument("--contract-version", type=int, default=1)
    args = parser.parse_args()
    try:
        reply = discover(args.engine, args.require, args.contract_version)
    except (RuntimeContractError, OSError, subprocess.SubprocessError) as error:
        print(f"spark_runtime: {error}", file=sys.stderr)
        return 2
    print(json.dumps(reply, indent=2))
    return 0


if __name__ == "__main__":
    sys.exit(main())
