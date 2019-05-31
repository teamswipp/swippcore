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
import { OutputFactory } from "javascript-terminal";
import Highlight from "react-highlight";
import "highlight.js/styles/nord.css";
import "./json-output.css";

export const JSON_TYPE = "json";

export const JsonOutput = ({content}) => (
	<Highlight language="json">
		{JSON.stringify(content, null, 4)}
	</Highlight>
);

export const makeJsonRecord = (text) => {
	return new OutputFactory.OutputRecord({
		type: JSON_TYPE,
		content: text
	});
};

