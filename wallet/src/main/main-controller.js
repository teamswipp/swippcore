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

import { BrowserWindow, shell } from "electron";
import electron from "electron";

export default class MainController {
	constructor() {
		this.window = MainController.create_window();
	}

	static create_window() {
		var window = new BrowserWindow({
			width: parseInt(electron.screen.getPrimaryDisplay().workAreaSize.height / 1.25),
			height: electron.screen.getPrimaryDisplay().workAreaSize.height / 2,
			minWidth: 750,
			minHeight: 500,
			frame: false,
			resizable: true,
			show: false
		});

		if (global.isDevelopment) {
			window.webContents.openDevTools({ mode : "detach" });
			window.loadURL(`http://localhost:${process.env.ELECTRON_WEBPACK_WDS_PORT}?route=main`);
		} else {
			window.loadURL(formatUrl({
				pathname: path.join(__dirname, "main-window.html?route=main"),
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
		});

		window.webContents.on('new-window', (event, url) => {
			event.preventDefault();
			shell.openExternal(url);
		});

		return window;
	}
}

