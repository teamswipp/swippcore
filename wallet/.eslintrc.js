module.exports = {
  /* your base configuration of choice */

  parser: 'babel-eslint',
  parserOptions: {
    "ecmaVersion": 8,
    "sourceType": "module",
    "ecmaFeatures": {
      "jsx": true
    }
  },
  env: {
    browser: true,
    node: true
  },
  globals: {
    __static: true
  }
}
