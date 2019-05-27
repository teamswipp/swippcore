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
import { ReactOutputRenderer, ReactTerminal, ReactThemes } from "react-terminal-component";
import { CommandMapping, defaultCommandMapping, EmulatorState, OutputFactory} from "javascript-terminal";
import Content from "../content";
import "./cli-content.css";

export default class CLIContent extends React.Component {
	constructor(props) {
		super(props);
	}

	render() {
		//TODO: We need to handle this somehow
		const customState = EmulatorState.create({
			"commandMapping": CommandMapping.create({
				/*...defaultCommandMapping,*/
				"*": {
					"function": (state, opts) => {
						const input = opts.join(" ");

						return {
							output: OutputFactory.makeTextOutput(input)
						};
					},
					'optDef': {}
				}
			})
		});

		return(
			<Content id="cli">
				<ReactTerminal emulatorState={customState} theme={ReactThemes.sea} promptSymbol={"swippd > "} />
			</Content>
		);
	}
}

