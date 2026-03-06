const MiniCssExtractPlugin = require('mini-css-extract-plugin');
const CopyPlugin = require('copy-webpack-plugin');

// Extract CSS
const extractCSS = new MiniCssExtractPlugin({
  filename: '[name].css'
});

// Copy file
const copyPlugin = new CopyPlugin({
  patterns: [{
    from: 'assets/*',
    to: ''
  }],
});

module.exports = {
  entry: {
    qrenc: './js/qrenc.js',
    qrdec: './js/qrdec.js',
  },
  output: {
    path: __dirname + '/dist',
    filename: '[name].js'
  },
  module: {
    rules: [{
      test: /\.css$/,
      use: [
        MiniCssExtractPlugin.loader,
        'css-loader',
        'postcss-loader'
      ]
    }, {
      test: /\.(woff|woff2|eot|ttf|otf)$/,
      type: 'asset/resource',
      generator: {
        filename: 'assets/[hash][ext]'
      }
    }]
  },
  devServer: {
    static: __dirname
  },
  plugins: [
    extractCSS,
    copyPlugin
  ]
}
