#!/usr/bin/env node
/**
 * conform-gate — PostToolUse on Edit|Write.
 *
 * After each edit of a C source/header, runs the DETERMINISTIC contract<->code
 * conform check (slice1-v2/conform/check.py, 0 tokens, no LLM) on the region the
 * file belongs to. If contract and code have drifted:
 *   - drift (1..N-1 edits)  → systemMessage: "reconcile before adding more"
 *   - halt  (>=N edits)     → block + reason: "STOP, you are looping on a drifted base"
 *
 * Purpose: stop an uncommitted agent from flailing for hours on a drifted base
 * (the daf35d0 false-success class). The signal comes from OUTSIDE the agent's
 * loop and interrupts it. fail-open on any error.
 */
import { execFileSync } from "node:child_process";
import path from "node:path";
import { fileURLToPath } from "node:url";

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const CHECK = path.join(__dirname, "..", "..", "slice1-v2", "conform", "check.py");

async function main() {
	let input = "";
	for await (const c of process.stdin) input += c;
	let d;
	try {
		d = JSON.parse(input);
	} catch {
		process.exit(0);
	}
	const tn = d.tool_name || "";
	if (tn !== "Edit" && tn !== "Write") process.exit(0);
	const fp = d.tool_input?.file_path || "";
	if (!fp || !/\.(c|h)$/.test(fp)) process.exit(0); // only C sources/headers

	let out;
	try {
		out = execFileSync("python3", [CHECK, "--file", fp], { encoding: "utf8", timeout: 10000 });
	} catch {
		process.exit(0); // python missing / check error → fail-open
	}
	let v;
	try {
		v = JSON.parse(out.trim().split("\n").pop());
	} catch {
		process.exit(0);
	}

	if (v.status === "halt") {
		process.stdout.write(JSON.stringify({ decision: "block", reason: v.message }));
	} else if (v.status === "drift") {
		process.stdout.write(JSON.stringify({ systemMessage: v.message }));
	}
	process.exit(0);
}

main().catch(() => process.exit(0));
