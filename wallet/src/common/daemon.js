import { execFile } from "child_process";
import crypto from "crypto";

const isDevelopment = process.env.NODE_ENV !== "production";

export default class Daemon {
	static execute(window, location) {
		var clargs = process.argv.slice(isDevelopment ? 3 : 1);

		process.credentials = {
			user: crypto.randomBytes(6).toString('hex'),
			password: crypto.randomBytes(20).toString('hex')
		};

		clargs.push(`-rpcuser=${process.credentials.user}` , `-rpcpassword=${process.credentials.password}`);

		execFile(location, clargs, { windowsHide: true }, (error, stdout, stderr) => {
			if (error) {
				window.webContents.send("fatal-error", stderr);
				window.webContents.send("state", "idle");
				console.error(stderr);
			}
		});
	}

	static start(window) {
		if (isDevelopment) {
			Daemon.execute(window, "../daemon/swippd");

		} else {
			Daemon.execute(window, process.resourcesPath + "/../swippd");
		}
	}
}

