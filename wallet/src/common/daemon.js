/*
 * This file is part of The Swipp Wallet
 * Copyright (C) 2019 The Swipp developers <info@swippcoin.com>
 *
 * The Swipp Wallet is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Swipp Wallet is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with The Swipp Wallet. If not, see <https://www.gnu.org/licenses/>.
 */

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

