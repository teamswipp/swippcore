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

import { Client } from "node-json-rpc2";

export default class RPCClient {
	constructor() {
		this.client = new Client({
			user: global.credentials.user,
			password: global.credentials.password,
			port: global.rpcPort /* Should have been initialized by daemon */
		});
	}

	async getinfo() {
		var client = this.client;

		return await new Promise((resolve, reject) => {
			client.call({method: "getinfo" }, (err, response) => {
				if(err) {
					reject(err);
				}

				resolve(response.result);
			});
		});
	}
}

