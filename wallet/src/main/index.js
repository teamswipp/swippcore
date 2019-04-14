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

import { app, BrowserWindow, ipcMain } from "electron";
import * as path from "path";
import { format as formatUrl } from "url";
import Daemon from "common/daemon";
import SplashController from "./splash-controller";

const isDevelopment = process.env.NODE_ENV !== "production";

if (app.getGPUFeatureStatus().gpu_compositing.includes("disabled")) {
	app.disableHardwareAcceleration();
}

// Necessary to prevent window from being garbage collected
let mainWindow;
let splashWindow;

function createSplashWindow() {
	const window = new BrowserWindow({
		width: 600,
		height: 236,
		frame: false,
		resizable: false,
		show: false
	});

	if (isDevelopment) {
		window.webContents.openDevTools({ mode : "detach" });
		window.loadURL(`http://localhost:${process.env.ELECTRON_WEBPACK_WDS_PORT}`);
	} else {
		window.loadURL(formatUrl({
			pathname: path.join(__dirname, "index.html"),
			protocol: "file",
			slashes: true
		}));
  	}

	window.on("closed", () => {
		splashWindow = null
	})

	window.webContents.on("devtools-opened", () => {
		window.focus();
		setImmediate(() => {
			window.focus();
		});
	});

	return window;
}

function createMainWindow() {
}

app.on("window-all-closed", () => {
	if (process.platform !== "darwin") {
		app.quit();
	}
});

app.on("activate", () => {
	if (mainWindow === null) {
		mainWindow = createMainWindow();
	}
});

app.on("ready", () => {
	splashWindow = createSplashWindow();

	splashWindow.webContents.on("did-finish-load", () => {
		splashWindow.show();
		splashWindow.webContents.send("state", "working");

		Daemon.start(splashWindow).then(function() {
			new SplashController(splashWindow).synchronize_wallet();
		}, function(stderr) {
			console.error(stderr);
		});
	});
});

ipcMain.on("exit", () => {
	app.quit()
});
