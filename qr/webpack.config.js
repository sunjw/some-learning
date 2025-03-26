const MiniCssExtractPlugin = require('mini-css-extract-plugin');
// Extract CSS
const extractCSS = new MiniCssExtractPlugin({
  filename: 'styles.min.css'
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
    extractCSS
  ]
}
