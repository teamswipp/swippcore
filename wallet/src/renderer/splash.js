import React from "react";
import { ipcRenderer } from "electron";
import { library } from "@fortawesome/fontawesome-svg-core";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { faSpinner, faTimes } from "@fortawesome/free-solid-svg-icons";
import File from "common/file";
import "./splash.css"

library.add(faSpinner, faTimes)

export default class Splash extends React.Component {
	constructor(props) {
		super(props);
		this.state = { errorMessage: null };

		ipcRenderer.on("fatal-error", (event, message) => {
			this.setState({ errorMessage: message });
		});
	}

	render() { return(
		<div className="splash">
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
				<div className="error">{this.state.errorMessage}</div>
			</div>
		</div>
	);}
}
