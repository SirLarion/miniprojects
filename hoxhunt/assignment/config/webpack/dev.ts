import path from 'path';
import webpack from 'webpack';
import HtmlWebpackPlugin from 'html-webpack-plugin';

const config: webpack.Configuration = {
	mode: 'development',
	output: {
		publicPath: '/',
	},
	entry: './src/app/index.tsx',
	module: {
		rules: [
			{
				test: /\.(ts|js)x?$/i,
				exclude: /node_modules/,
				use: {
					loader: 'babel-loader',
					options: {
						presets: ['@babel/preset-env', '@babel/preset-react', '@babel/preset-typescript'],
					},
				},
			},
			{
				test: /\.css$/,
				use: ['style-loader', 'css-loader']
			},

		],
	},
	resolve: {
		extensions: ['.tsx', '.ts', '.js'],
	},
	plugins: [
		new HtmlWebpackPlugin({
			template: 'src/app/index.html',
		}),
		new webpack.HotModuleReplacementPlugin(),
	],
	devtool: 'inline-source-map',
	devServer: {
		historyApiFallback: true,
		port: 8080,
		open: true,
		hot: true,
	},
};

export default config;
