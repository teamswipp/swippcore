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

import { rotatePoint } from "./helpers";

export default class Bullet {
	constructor(args) {
		let posDelta = rotatePoint(
			{x: 0, y: -20},
			{x: 0, y: 0},
			args.ship.rotation * Math.PI / 180
		);

		this.position = {
			x: args.ship.position.x + posDelta.x,
			y: args.ship.position.y + posDelta.y
		};

		this.rotation = args.ship.rotation;
		this.velocity = { x: posDelta.x / 2, y: posDelta.y / 2 };
		this.radius = 2;
	}

	destroy() {
		this.delete = true;
	}

	render(state) {
		this.position.x += this.velocity.x;
		this.position.y += this.velocity.y;

		// Out of bounds ?
		if ( this.position.x < 0 || this.position.y < 0 || this.position.x > state.screen.width
		     || this.position.y > state.screen.height ) {
			this.destroy();
		}

		const context = state.context;

		context.save();
		context.translate(this.position.x, this.position.y);
		context.rotate(this.rotation * Math.PI / 180);
		context.fillStyle = "#fff";
		context.lineWidth = 0,5;

		context.beginPath();
		context.arc(0, 0, 2, 0, 2 * Math.PI);
		context.closePath();

		context.fill();
		context.restore();
	}
}

