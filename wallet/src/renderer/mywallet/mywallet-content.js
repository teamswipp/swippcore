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
import Select from "react-select";
import Store from "electron-store";
import { Helmet } from "react-helmet";
import CoinGecko from "common/coingecko.js";
import RPCClient from "common/rpc-client.js";
import Content from "../content";
import "./mywallet-content.css";

export default class MyWalletContent extends React.Component {
	constructor(props) {
		super(props);
		var coinGecko = new CoinGecko();
		var rpcClient = new RPCClient();

		this.store = new Store();

		this.state = {
			balance: 0,
			currencies: [],
			selectedCurrency: []
		};

		Promise.all([
			rpcClient.getbalance(),
			coinGecko.getsupported(),
		]).then((response) => {
			coinGecko.getprice("swipp", response[1]).then((rates) => {
				this.state.selectedCurrency = this.store.get("currency", "usd");

				var currencies = Object.entries(rates.swipp).map((currency) => {
					var v = { value: currency[0], label: currency[0], rate: currency[1] };

					if (currency[0] == this.state.selectedCurrency) {
						this.setState({ selectedCurrency: v });
					}

					return v;
				});

				this.setState({ balance: response[0], currencies: currencies });
			});
		});
	}

	render() {
		var onChange = (e) => {
			this.setState({ selectedCurrency: e });
			this.store.set("currency", e.value);
		}

		return(
			<Content id="mywallet">
				<Helmet>
					<script src="https://widgets.coingecko.com/coingecko-coin-price-marquee-widget.js" />
				</Helmet>
				<coingecko-coin-price-marquee-widget coin-ids="swipp,bitcoin,litecoin,dogecoin"
				                                     currency={this.state.selectedCurrency.value}
				                                     background-color="#000" locale="en" />
				<div>
					<h1>{this.state.balance} SWIPP</h1>
					<h2>
						Valued at {this.state.balance * this.state.selectedCurrency.rate}
						<Select className="select" classNamePrefix="select" options={this.state.currencies}
						        value={this.state.selectedCurrency} onChange={onChange} />
					</h2>
				</div>
			</Content>
		);
	}
}

