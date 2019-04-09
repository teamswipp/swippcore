"use strict";

import { app, BrowserWindow } from "electron";
import * as path from "path";
import { format as formatUrl } from "url";

const isDevelopment = process.env.NODE_ENV !== "production";

if (app.getGPUFeatureStatus().gpu_compositing.includes("disabled")) {
	app.disableHardwareAcceleration();
}

// Necessary to prevent window from being garbage collected
let mainWindow;

function createSplashWindow() {
	const window = new BrowserWindow({
		width: 600,
		height: 200,
		frame: false,
		resizable: false
	});

	if (isDevelopment) {
		window.webContents.openDevTools({ mode : "detach" });
		window.loadURL(`http://localhost:${process.env.ELECTRON_WEBPACK_WDS_PORT}`);
	} else {
		window.loadURL(formatUrl({
			pathname: path.join(__dirname, 'index.html'),
			protocol: 'file',
			slashes: true
		}));
  	}

	window.on("closed", () => {
		mainWindow = null
	})

	window.webContents.on('devtools-opened', () => {
		window.focus();
		setImmediate(() => {
			window.focus();
		});
	});

	return window;
}

app.on("window-all-closed", () => {
	if (process.platform !== "darwin") {
		app.quit();
	}
});

app.on("activate", () => {
	if (mainWindow === null) {
		mainWindow = createSplashWindow();
	}
});

app.on("ready", () => {
	mainWindow = createSplashWindow();
});

