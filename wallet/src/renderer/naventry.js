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
import { ipcRenderer, remote } from "electron";
import "./naventry.css"

export default class NavEntry extends React.Component {
	constructor(props) {
		super(props);

		this.state = {
			active: this.props.children[0]._owner.pendingProps.active
		};

		ipcRenderer.on("navigate", (event, source) => {
			if (source != this.props.children[1]) {
				this.setState({ active: false });
			}
		});
	}

	render() {
		var onClick = (e) => {
			ipcRenderer.sendTo(remote.getCurrentWebContents().id, "navigate", this.props.children[1]);
			this.setState({ active: true });
		}

		return(
			<li onClick={onClick} className={this.state.active ? "active " : "inactive " + this.props.className}>
				{this.props.children}
			</li>
		);
	}
}

