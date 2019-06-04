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

import Particle from "./particle";
import { asteroidVertices, randomNumBetween } from "./helpers";

export default class Asteroid {
	constructor(args) {
		this.velocity = {
			x: randomNumBetween(-1.5, 1.5),
			y: randomNumBetween(-1.5, 1.5)
		};

		this.position = args.position;
		this.rotation = 0;
		this.rotationSpeed = randomNumBetween(-1, 1);
		this.radius = args.size;
		this.score = (80 / this.radius) * 5;
		this.create = args.create;
		this.addScore = args.addScore;
		this.vertices = asteroidVertices(8, args.size);
	}

	destroy() {
		this.delete = true;
		this.addScore(this.score);

		// Explode
		for (let i = 0; i < this.radius; i++) {
			const particle = new Particle({
				lifeSpan: randomNumBetween(60, 100),
				size: randomNumBetween(1, 3),
				position: {
					x: this.position.x + randomNumBetween(-this.radius / 4, this.radius / 4),
					y: this.position.y + randomNumBetween(-this.radius / 4, this.radius / 4)
				}, velocity: {
					x: randomNumBetween(-1.5, 1.5),
					y: randomNumBetween(-1.5, 1.5)
				}
			});

			this.create(particle, "particles");
		}

		// Break into smaller pieces
		if(this.radius > 10) {
			for (let i = 0; i < 2; i++) {
				let asteroid = new Asteroid({
					size: this.radius/2,
					position: {
						x: randomNumBetween(-10, 20) + this.position.x,
						y: randomNumBetween(-10, 20) + this.position.y
					}, create: this.create.bind(this),
					addScore: this.addScore.bind(this)
				});

				this.create(asteroid, 'asteroids');
			}
		}
	}

	render(state) {
		this.position.x += this.velocity.x;
		this.position.y += this.velocity.y;
		this.rotation += this.rotationSpeed;

		if (this.rotation >= 360) {
			this.rotation -= 360;
		}

		if (this.rotation < 0) {
			this.rotation += 360;
		}

		// Screen edges
		if (this.position.x > state.screen.width + this.radius) {
			this.position.x = -this.radius;
		} else if (this.position.x < -this.radius) {
			this.position.x = state.screen.width + this.radius;
		}

		if (this.position.y > state.screen.height + this.radius) {
			this.position.y = -this.radius;
		} else if (this.position.y < -this.radius) {
			this.position.y = state.screen.height + this.radius;
		}

		const context = state.context;

		context.save();
		context.translate(this.position.x, this.position.y);
		context.rotate(this.rotation * Math.PI / 180);
		context.strokeStyle = "#fff";
		context.lineWidth = 2;

		context.beginPath();
		context.moveTo(0, -this.radius);

		for (let i = 1; i < this.vertices.length; i++) {
			context.lineTo(this.vertices[i].x, this.vertices[i].y);
		}

		context.closePath();
		context.stroke();
		context.restore();
	}
}

