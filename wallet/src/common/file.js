import fs from "fs";
import path from "path";

const isDevelopment = process.env.NODE_ENV !== "production";

export default class File {
	static get(f) {
		if (isDevelopment) {
			return f;
		}

		return path.join(__static, f);
	}
}

