import { execFile } from "child_process";

const isDevelopment = process.env.NODE_ENV !== "production";

export default class Daemon {
	static execute(window, location) {
		execFile(location, [], { windowsHide: true }, (error, stdout, stderr) => {

			if (error) {
				window.webContents.send("fatal-error", stderr);
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

