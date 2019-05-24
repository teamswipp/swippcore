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
import { Helmet } from "react-helmet";
import RPCClient from "common/rpc-client.js"
import Content from "../content";
import "./mywallet-content.css";

export default class MyWalletContent extends React.Component {
	constructor(props) {
		super(props);
		this.state = { balance: 0 };
		var rpcClient = new RPCClient();

		rpcClient.getbalance().then((response) => {
		});

		Promise.all([
			rpcClient.getbalance()
		]).then((response) => {
			this.setState({ balance: response[0] });
		});
	}

	render() {
		return(
			<Content id="mywallet">
				<Helmet>
					<script src="https://widgets.coingecko.com/coingecko-coin-price-marquee-widget.js" />
				</Helmet>
				<coingecko-coin-price-marquee-widget coin-ids="swipp,bitcoin,litecoin,dogecoin" currency="usd" background-color="#000" locale="en" />
				<div className="balance content">
					<div>{this.state.balance} SWIPP</div>
				</div>
			</Content>
		);
	}
}

