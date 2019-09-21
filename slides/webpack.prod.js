const merge = require('webpack-merge');
const webpack = require('webpack');
const UglyfyJsPlugin = require('uglifyjs-webpack-plugin');
const MiniCssExtractPlugin = require('mini-css-extract-plugin');
const OptiizeCssAssetsPlugin = require('optimize-css-assets-webpack-plugin');

module.exports = merge(require('./webpack.common'), {

  mode: 'production',
  devtool: 'source-map',

  module: {
    rules: [
      {
        test: /\.(sa|sc|c)ss$/,
        use: [
          MiniCssExtractPlugin.loader,
          'css-loader',
          {
            loader: 'postcss-loader',
            options: {
              plugins: [require('postcss-preset-env')]
            }
          },
          'sass-loader'
        ]
      }
    ]
  },

  output: {
    filename: 'assets/[name].[contenthash].js'
  },

  plugins: [
    new MiniCssExtractPlugin({
      filename: 'assets/[name].[contenthash].css'
    }),
    new webpack.HashedModuleIdsPlugin()
  ],

  optimization: {
    minimizer: [
      new UglyfyJsPlugin({cache: true, parallel: true, sourceMap: true}),
      new OptiizeCssAssetsPlugin({})
    ]
  }

});
