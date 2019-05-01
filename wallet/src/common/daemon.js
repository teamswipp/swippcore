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
import emptyPromise from "empty-promise";
import portscanner from "portscanner";
import tcpPortUsed from "tcp-port-used";

const DEFAULT_RPC_PORT = 35075;
const DAEMON_DONE_PROMISE = emptyPromise();

export default class Daemon {
	static execute(window, location, downloadBootstrap) {
		return new Promise((resolve, reject) => {
			var clargs = process.argv.slice(global.isDevelopment ? 3 : 1);

			if (global.credentials == undefined)  {
				global.credentials = {
					user: crypto.randomBytes(6).toString('hex'),
					password: crypto.randomBytes(20).toString('hex')
				};
			}

			clargs.push(`-rpcuser=${global.credentials.user}` , `-rpcpassword=${global.credentials.password}`);

			if (downloadBootstrap) {
				clargs.push("-loadblock=web");
			}

			portscanner.findAPortNotInUse(DEFAULT_RPC_PORT, DEFAULT_RPC_PORT + 1024,
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
				}).on("exit", () => {
					DAEMON_DONE_PROMISE.resolve();
				});

				tcpPortUsed.waitUntilUsed(global.rpcPort, 200, 5000).then(() => {
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

	static async start(window, downloadBootStrap = false) {
		var p;

		if (isDevelopment) {
			p = await Daemon.execute(window, "../daemon/swippd", downloadBootStrap);
		} else {
			p = await Daemon.execute(window, process.resourcesPath + "/../swippd", downloadBootStrap);
		}

		return p;
	}

	static async done() {
		return await DAEMON_DONE_PROMISE;
	}
}

