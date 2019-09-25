const path = require('path');
const {CleanWebpackPlugin} = require('clean-webpack-plugin');
const HtmlPlugin = require('html-webpack-plugin');
const webpack = require('webpack');

module.exports = {

  entry: './src/index.js',

  module: {
    rules: [
      {
        test: /\.(ttf|otf|eot|svg|woff(2)?|png|mp4|cast)$/,
        loader: 'file-loader',
        options: {
          name: 'assets/[hash].[ext]'
        }
      },
      {
        test: /\.ejs$/,
        loader: 'ejs-loader'
      }
    ]
  },

  plugins: [
    new CleanWebpackPlugin(),
    new webpack.DefinePlugin({
      REPORT_IMAGES_DIR: JSON.stringify(path.resolve(__dirname, '../report/images') + '/')
    }),
    new webpack.ProvidePlugin({
      React: 'react'  // Cause asciinema-player really sucks
    }),
    new HtmlPlugin({
      title: 'Parallelizing Mathematical Morphology · Slides',
      template: './src/index.ejs',
      minify: {
        caseSensitive: true,
        collapseWhitespace: true,
        removeComments: true,
        removeRedundantAttributes: true,
        preserveLineBreaks: false
      }
    })
  ],

  output: {
    filename: '[name].bundle.js',
    path: path.resolve(__dirname, 'dist')
  }

};
