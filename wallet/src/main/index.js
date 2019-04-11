import { app, BrowserWindow, ipcMain } from "electron";
import * as path from "path";
import { format as formatUrl } from "url";
import Daemon from "common/daemon";

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
		Daemon.start(splashWindow);
	});
});

ipcMain.on("exit", () => {
	app.quit()
});
