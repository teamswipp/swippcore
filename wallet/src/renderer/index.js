import React from "react";
import ReactDOM from "react-dom";
import { library } from '@fortawesome/fontawesome-svg-core'
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome'
import { faSpinner } from '@fortawesome/free-solid-svg-icons'
import File from "common/file";
import "./splash.css"

library.add(faSpinner)

ReactDOM.render(
	<div className="splash">
		<FontAwesomeIcon icon="spinner" color="white" spin />
		<img className="logo" src={File.get("logo.png")}></img>
		<div>
			<h1>The modern cryptocurrency</h1>
			<h2>Copyright © 2017-2019 The Swipp Developers</h2>
			<p>
				This project is using code based on the work of
				<strong> Satoshi Nakamoto</strong>,
				<strong> The Bitcoin Developers</strong>,
				<strong> The Peercoin Developers</strong>,
				<strong> The Novacoin Developers</strong> and
				<strong> The Darkcoin Developers</strong>,
				based on work between 2009 and 2017.
			</p>
		</div>
	</div>,
	document.getElementById("app")
);

