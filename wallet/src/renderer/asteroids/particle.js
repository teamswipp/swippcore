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

export default class Particle {
	constructor(args) {
		this.position = args.position;
		this.velocity = args.velocity;
		this.radius = args.size;
		this.lifeSpan = args.lifeSpan;
		this.inertia = 0.98;
	}

	destroy() {
		this.delete = true;
	}

	render(state) {
		this.position.x += this.velocity.x;
		this.position.y += this.velocity.y;
		this.velocity.x *= this.inertia;
		this.velocity.y *= this.inertia;

		this.radius -= 0.1;

		if (this.radius < 0.1) {
			this.radius = 0.1;
		}

		if (this.lifeSpan-- < 0) {
			this.destroy();
		}

		const context = state.context;

		context.save();
		context.translate(this.position.x, this.position.y);
		context.fillStyle = "#fff";
		context.lineWidth = 2;

		context.beginPath();
		context.moveTo(0, -this.radius);
		context.arc(0, 0, this.radius, 0, 2 * Math.PI);
		context.closePath();

		context.fill();
		context.restore();
	}
}

