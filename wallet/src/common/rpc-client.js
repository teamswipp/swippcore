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
import { remote } from "electron";

export default class RPCClient {
	constructor() {
		var credentials = global.credentials == undefined ? remote.getGlobal("credentials") : global.credentials;
		var rpcPort = global.rpcPort == undefined ? remote.getGlobal("rpcPort") : global.rpcPort;

		this.client = new Client({
			user: credentials.user,
			password: credentials.password,
			port: rpcPort /* Should have been initialized by daemon */
		});

		this.send_command = (command, args = []) => {
			return new Promise((resolve, reject) => {
				this.client.call({ method: command, params: args }, (err, response) => {
					if(err) {
						reject(err);
					} else {
						resolve(response.result);
					}
				});
			});
		}
	}

	async getbalance() {
		return await this.send_command("getbalance");
	}

	async getblockcount() {
		return await this.send_command("getblockcount");
	}

	async getinfo() {
		return await this.send_command("getinfo");
	}

	async stop() {
		return await this.send_command("stop");
	}
}

