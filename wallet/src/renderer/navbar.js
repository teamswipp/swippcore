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
import MyWallet from "./mywallet/mywallet.js";
import Transactions from "./transactions/transactions.js";
import Addresses from "./addresses/addresses.js";
import Settings from "./settings/settings.js";
import "./navbar.css"

export default class NavBar extends React.Component {
	constructor(props) {
		super(props);
		this.state = {};

		if (this.props.active == "undefined") {
			this.state.active = false;
		} else {
			this.state.active = this.props.active;
		}

		var onClick = (e) => {
			/*if (this.state.active == true)
				console.log(e);*/
		}

		this.entries = [
			<MyWallet key="mywallet" active={true} onClick={onClick} />,
			<Addresses key="addresses" onClick={onClick} />,
			<Transactions key="transactions" onClick={onClick} />,
			<Settings key={"settings"} className="bottom" onClick={onClick} />
		];
	}

	render() {
		return(
			<nav>
				<ul>
					{this.entries}
				</ul>
			</nav>
		);
	}
}

