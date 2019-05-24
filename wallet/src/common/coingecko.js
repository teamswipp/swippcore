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

import https from "https";

export default class CoinGecko {
	constructor() {
		this.send_command = (command, args = []) => {
			return new Promise((resolve, reject) => {
				https.get("https://api.coingecko.com/api/v3/" + command, (resp) => {
					let data = "";

					resp.on("data", (chunk) => {
						data += chunk;
					});

					resp.on("end", () => {
						try {
							resolve(JSON.parse(data));
						} catch(exception) {
							reject(exception);
						}
					});
				}).on("error", (err) => {
					reject(err.message);
				});
			});
		}
	}

	async getprice(id, currencies) {
		return await this.send_command(`simple/price?ids=${id}&vs_currencies=${encodeURI(currencies)}`);
	}

	async getrates() {
		return await this.send_command("exchange_rates");
	}

	async getsupported() {
		return await this.send_command("simple/supported_vs_currencies");
	}
}

