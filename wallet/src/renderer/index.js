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
import ReactDOM from "react-dom";
import { Router } from "common/router";
import ControlBar from "./controlbar";
import NavBar from "./navbar";
import Splash from "./splash";
import MyWalletContent from "./mywallet/mywallet-content.js";
import TransactionsContent from "./transactions/transactions-content.js";
import AddressesContent from "./addresses/addresses-content.js";
import SettingsContent from "./settings/settings-content.js";
import CLIContent from "./cli/cli-content.js";
import "./index.css";

const routes = {
	main: [
		<ControlBar key={1} headerText="Swipp" fullControls={true} />,
		<div className="nav" key={2}>
			<NavBar />
			<div>
				<MyWalletContent key="mywallet-content" active={true} />
				<AddressesContent key="addressbook-content" />
				<TransactionsContent key="transactions-content" />
				<SettingsContent key="settings-content" />
				<CLIContent key="cli-content" />
			</div>
		</div>
	],
	splash: [
		<ControlBar key={1} className="nobg" fullControls={false} />, <Splash key={2} />
	]
};

ReactDOM.render (<Router routes={routes} />, document.getElementById ("app"));

