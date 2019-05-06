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

import { app, ipcMain } from "electron";
import * as path from "path";
import { format as formatUrl } from "url";
import Daemon from "common/daemon";
import RPCClient from "common/rpc-client.js"
import SplashController from "./splash-controller";

global.isDevelopment = process.env.NODE_ENV !== "production";

if (app.getGPUFeatureStatus().gpu_compositing.includes("disabled")) {
	app.disableHardwareAcceleration();
}

app.on("window-all-closed", () => {
	if (global.rpcPort != undefined) {
		new RPCClient().stop();
	}

	app.quit();
});

app.on("activate", () => {
/*	if (mainWindow === null) {
		mainWindow = createMainWindow();
	}*/
});

const defaultRPCPort = 35075;

app.on("ready", () => {
	var splashController = new SplashController();

	splashController.window.webContents.on("did-finish-load", () => {
		splashController.window.webContents.send("progress", "indeterminate", "Initializing Swipp daemon...");

		Daemon.start(splashController.window).then(() => {
			var rpcClient = new RPCClient();

			splashController.version_control(rpcClient).then(() => {
				splashController.synchronize_wallet(rpcClient).then(() => {
					/* If sync was a success, we close the splash and move on to the main wallet window */
				}, (stderr) => {
					console.error(stderr);
				});
			}, (stderr) => {
				console.error(stderr);
			});
		}, (stderr) => {
			console.error(stderr);
		});
	});
});

