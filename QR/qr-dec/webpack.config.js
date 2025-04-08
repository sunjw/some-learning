const MiniCssExtractPlugin = require('mini-css-extract-plugin');
const CopyPlugin = require('copy-webpack-plugin');

// Extract CSS
const extractCSS = new MiniCssExtractPlugin({
  filename: 'styles.min.css'
});

// Copy file
const copyPlugin = new CopyPlugin({
  patterns: [{
    from: 'assets/*',
    to: ''
  }],
});

module.exports = {
  entry: './js/main.js',
  output: {
    path: __dirname + '/dist',
    filename: 'bundle.js'
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
