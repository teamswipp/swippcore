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
import portscanner from "portscanner";
import tcpPortUsed from "tcp-port-used";

const defaultRPCPort = 35075;

export default class Daemon {
	static execute(window, location) {
		return new Promise((resolve, reject) => {
			var clargs = process.argv.slice(global.isDevelopment ? 3 : 1);

			global.credentials = {
				user: crypto.randomBytes(6).toString('hex'),
				password: crypto.randomBytes(20).toString('hex')
			};

			clargs.push(`-rpcuser=${global.credentials.user}` , `-rpcpassword=${global.credentials.password}`);

			portscanner.findAPortNotInUse(defaultRPCPort, defaultRPCPort + 1024,
			                              "127.0.0.1", (error, port) => {
				var executionError = false;

				global.rpcPort = port;
				clargs.push(`-rpcport=${port}`);

				execFile(location, clargs, { windowsHide: true }, (error, stdout, stderr) => {
					if (error) {
						executionError = true;
						window.webContents.send("fatal-error", stderr);
						window.webContents.send("state", "idle");
						reject(stderr);
					}
				});

				tcpPortUsed.waitUntilUsed(global.rpcPort, 200, 3000).then(() => {
					resolve();
				}, (err) => {
					if (!executionError) {
						var errorMessage = `Error waiting for the wallet daemon: ${err.message}`;
						window.webContents.send("fatal-error", errorMessage);
						window.webContents.send("state", "idle");
						reject(errorMessage);
					}
				});
			});
		});
	}

	static async start(window) {
		var p;

		if (isDevelopment) {
			p = await Daemon.execute(window, "../daemon/swippd");
		} else {
			p = await Daemon.execute(window, process.resourcesPath + "/../swippd");
		}

		return p;
	}
}

