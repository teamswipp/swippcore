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

import React from "react";
import { ipcRenderer } from "electron";
import { library } from "@fortawesome/fontawesome-svg-core";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { faSpinner, faTimes } from "@fortawesome/free-solid-svg-icons";
import File from "common/file";
import "./splash.css"

library.add(faSpinner, faTimes)

export default class Splash extends React.Component {
	constructor(props) {
		super(props);
		this.state = {
			infoMessage: null,
			errorMessage: null,
			percentage: "indeterminate"
		};

		ipcRenderer.on("fatal-error", (event, message) => {
			this.setState({ infoMessage: null, errorMessage: message });
		});

		ipcRenderer.on("progress", (event, percentage, message) => {
			this.setState({ infoMessage: message, errorMessage: null, percentage: percentage });
		});
	}

	render() {
		return(
			<div className="splash">
				<img className="logo" src={File.get("logo.png")}></img>
				<div>
					<h1>The modern cryptocurrency</h1>
					<h2>Copyright © 2017-2019 The Swipp Developers</h2>
					<p>
						This project is using code based on the work of
						<strong> Satoshi Nakamoto</strong>,
						<strong> The Bitcoin Developers</strong>,
						<strong> The Peercoin Developers</strong>,
						<strong> The Novacoin Developers</strong> and
						<strong> The Darkcoin Developers</strong>,
						based on work between 2009 and 2017.
					</p>
					<div className="error">{this.state.errorMessage}</div>
					{this.state.infoMessage != null && this.state.percentage == "indeterminate" &&
						<progress />
					}
					{this.state.infoMessage != null && this.state.percentage != "indeterminate" &&
						<progress value={this.state.percentage} />
					}
					<p>{this.state.infoMessage}</p>
				</div>
			</div>
		);
	}
}

