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

import { BrowserWindow } from "electron";
import { remote } from "electron";
import tcpPortUsed from "tcp-port-used";
import Daemon from "common/daemon";
import Explorer from "common/explorer";
import RPCClient from "common/rpc-client.js"
import Version from "common/version";

const BOOTSTRAP_DOWNLOAD_THRESHOLD_BLOCKS = 5000;

export default class SplashController {
	constructor() {
		this.window = SplashController.create_window();
	}

	static create_window() {
		var window = new BrowserWindow({
			width: 600,
			height: 236,
			frame: false,
			resizable: false,
			show: false
		});

		if (global.isDevelopment) {
			window.webContents.openDevTools({ mode : "detach" });
			window.loadURL(`http://localhost:${process.env.ELECTRON_WEBPACK_WDS_PORT}`);
		} else {
			window.loadURL(formatUrl({
				pathname: path.join(__dirname, "index.html"),
				protocol: "file",
				slashes: true
			}));
		}

		window.webContents.on("devtools-opened", () => {
			window.focus();
			setImmediate(() => {
				window.focus();
			});
		});

		window.webContents.on("did-finish-load", () => {
			window.show();
			window.webContents.send("state", "working");
		});

		return window;
	}

	async version_control(rpcClient) {
		return await new Promise((resolve, reject) => {
			rpcClient.getinfo().then((response) => {
				var latest_version = Version.get_cleaned_up(response["latest-version"]);

				if (Version.is_latest(response["version"], latest_version)) {
					resolve();
				} else {
					var errorMessage = "The daemon of this wallet is outdated. " +
					                   `Please download version ${latest_version}`;

					this.window.webContents.send("fatal-error", errorMessage);
					this.window.webContents.send("state", "idle");
					reject(errorMessage);
				}
			}, (stderr) => {
				reject(stderr);
			});
		});
	}

	async handle_synchronization(rpcClient) {
		this.window.webContents.send(
			"progress", "indeterminate", "Downloading and decompressing Swipp bootstrap archive..."
		);

		var startHeight = -1;
		var remoteHeight = 0;
		var localHeight = 0;
		var explorer = new Explorer();

		do {
			await new Promise((resolve, reject) => {
				Promise.all([
					rpcClient.getblockcount(), explorer.getblockcount(),
					new Promise(resolve => setTimeout(resolve, 500))
				]).then((response) => {
					localHeight = response[0];
					remoteHeight = response[1];

					if (startHeight == -1) {
						startHeight = localHeight;
					} else if (localHeight > startHeight) {
						this.window.webContents.send(
							"progress", localHeight / remoteHeight,
							`Synchronizing block ${localHeight} of ${remoteHeight}...`
						);
					}

					resolve();
				}, (stderr) => {
					console.error(stderr);
					reject();
				});
			});
		} while (remoteHeight > localHeight);
	}

	async synchronize_wallet(rpcClient) {
		this.window.webContents.send(
			"progress", "indeterminate", "Preparing wallet sycnhronization..."
		);

		return await new Promise((resolve, reject) => {
			Promise.all([rpcClient.getinfo(), new Explorer().getblockcount()]).then((response) => {
				/* Should we restart the daemon and download the bootstrap? */
				if (response[1] - response[0]["blocks"] > BOOTSTRAP_DOWNLOAD_THRESHOLD_BLOCKS) {
					Promise.all([rpcClient.stop(), Daemon.done()]).then((response) => {
						Daemon.start(this.window, true).then((response) => {
							this.handle_synchronization(rpcClient);
						});
					});
				} else {
					resolve();
				}
			}).catch((reason) => {
				reject();
			});
		});
	}
}

