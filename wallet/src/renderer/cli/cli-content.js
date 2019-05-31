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
import { ReactTerminalStateless, ReactOutputRenderers, ReactThemes } from "react-terminal-component";
import { CommandMapping, EmulatorState, OutputFactory, Outputs } from "javascript-terminal";
import Content from "../content";
import RPCClient from "common/rpc-client.js";
import CustomTerminal from "./custom-terminal";
import { JSON_TYPE, JsonOutput, makeJsonRecord } from "./json-output.js";
import "./cli-content.css";

export default class CLIContent extends React.Component {
	constructor(props) {
		super(props);
		this.rpcClient = new RPCClient();

		this.state = {
			terminal: EmulatorState.createEmpty(),
			inputStr: ""
		};
	}

	componentDidMount() {
		this.rpcClient.help().then((response) => {
			var commands = {};

			response.split("\n").forEach((line) => {
				var c = line.split(" ")[0];

				/* We return command results in onStateChange instead */
				commands[c] = { "function": () => {
					return { output: undefined }
				}, "optDef": {} }
			});

			const customState = EmulatorState.create({
				"commandMapping": CommandMapping.create(commands)
			});

			this.setState({ terminal: customState });
		});
	}

	render() {
		var onInputChange = (input) => {
			this.setState({ inputStr : input });
		}

		var onStateChange = (state, command) => {
			if (command != "" && state.getCommandMapping().get(command) != undefined) {
				this.rpcClient.raw_command(command, []).then((response) => {
					var msg;

					if (typeof response === "string") {
						msg = OutputFactory.makeTextOutput(response);
					} else {
						msg = makeJsonRecord(response);
					}

					const newState = state.setOutputs(Outputs.addRecord(state.getOutputs(), msg));
					this.setState({ terminal: newState, inputStr : "" });
				});
			} else {
				this.setState({ terminal: state, inputStr : "" });
			}
		}

		return(
			<Content id="cli">
				<CustomTerminal inputStr={this.state.inputStr} onInputChange={onInputChange} onStateChange={onStateChange}
				                emulatorState={this.state.terminal} theme={ReactThemes.sea} promptSymbol={"swippd > "}
				                outputRenderers={{...ReactOutputRenderers, [JSON_TYPE]: JsonOutput}} />
			</Content>
		);
	}
}

