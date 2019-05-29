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

import { ReactTerminalStateless } from "react-terminal-component";

export default class CustomTerminal extends ReactTerminalStateless {
	constructor(props) {
		super(props);

		this.emulator._addCommandOutputs = (state, outputs) => {
			for (const output of outputs) {
				var msg = output._values.get(1);

				if (typeof msg === "string" && msg.startsWith("emulator:")) {
					msg = msg.replace("emulator", "swippd")
					output._values = output._values.set(1, msg);
				}

				state = state.setOutputs(state.getOutputs().push(output));
			}

			return state;
		}

		super._submitInput = (commandStr) => {
			const {onStateChange, emulatorState} = this.props;
			const newState = this.emulator.execute(emulatorState, commandStr, this.plugins);
			onStateChange(newState, commandStr);
		}
	}
}

