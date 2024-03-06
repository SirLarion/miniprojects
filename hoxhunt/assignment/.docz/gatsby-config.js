const { mergeWith } = require('docz-utils')
const fs = require('fs-extra')

let custom = {}
const hasGatsbyConfig = fs.existsSync('./gatsby-config.custom.js')

if (hasGatsbyConfig) {
  try {
    custom = require('./gatsby-config.custom')
  } catch (err) {
    console.error(
      `Failed to load your gatsby-config.js file : `,
      JSON.stringify(err),
    )
  }
}

const config = {
  pathPrefix: '/',

  siteMetadata: {
    title: 'Components',
    description: 'Base project ',
  },
  plugins: [
    {
      resolve: 'gatsby-plugin-typescript',
      options: {
        isTSX: true,
        allExtensions: true,
      },
    },
    {
      resolve: 'gatsby-theme-docz',
      options: {
        themeConfig: {
          mode: 'light',
          showPlaygroundEditor: true,
          linesToScrollEditor: 14,
        },
        src: './',
        gatsbyRoot: null,
        themesDir: 'src',
        mdxExtensions: ['.md', '.mdx'],
        docgenConfig: {},
        menu: [],
        mdPlugins: [],
        hastPlugins: [],
        ignore: [],
        typescript: true,
        ts: false,
        propsParser: true,
        'props-parser': true,
        debug: false,
        native: false,
        openBrowser: null,
        o: null,
        open: null,
        'open-browser': null,
        root: '/home/sirlarion/repos/hoxhunt_pre-assignment/.docz',
        base: '/',
        source: './',
        'gatsby-root': null,
        files: '**/*.{md,markdown,mdx}',
        public: '/public',
        dest: '.docz/dist',
        d: '.docz/dist',
        editBranch: 'master',
        eb: 'master',
        'edit-branch': 'master',
        config: '',
        title: 'Components',
        description: 'Base project ',
        host: 'localhost',
        port: 9001,
        p: 3000,
        separator: '-',
        paths: {
          root: '/home/sirlarion/repos/hoxhunt_pre-assignment',
          templates:
            '/home/sirlarion/repos/hoxhunt_pre-assignment/node_modules/docz-core/dist/templates',
          docz: '/home/sirlarion/repos/hoxhunt_pre-assignment/.docz',
          cache: '/home/sirlarion/repos/hoxhunt_pre-assignment/.docz/.cache',
          app: '/home/sirlarion/repos/hoxhunt_pre-assignment/.docz/app',
          appPackageJson:
            '/home/sirlarion/repos/hoxhunt_pre-assignment/package.json',
          appTsConfig:
            '/home/sirlarion/repos/hoxhunt_pre-assignment/tsconfig.json',
          gatsbyConfig:
            '/home/sirlarion/repos/hoxhunt_pre-assignment/gatsby-config.js',
          gatsbyBrowser:
            '/home/sirlarion/repos/hoxhunt_pre-assignment/gatsby-browser.js',
          gatsbyNode:
            '/home/sirlarion/repos/hoxhunt_pre-assignment/gatsby-node.js',
          gatsbySSR:
            '/home/sirlarion/repos/hoxhunt_pre-assignment/gatsby-ssr.js',
          importsJs:
            '/home/sirlarion/repos/hoxhunt_pre-assignment/.docz/app/imports.js',
          rootJs:
            '/home/sirlarion/repos/hoxhunt_pre-assignment/.docz/app/root.jsx',
          indexJs:
            '/home/sirlarion/repos/hoxhunt_pre-assignment/.docz/app/index.jsx',
          indexHtml:
            '/home/sirlarion/repos/hoxhunt_pre-assignment/.docz/app/index.html',
          db: '/home/sirlarion/repos/hoxhunt_pre-assignment/.docz/app/db.json',
        },
        indexHtml: 'docz.html',
      },
    },
  ],
}

const merge = mergeWith((objValue, srcValue) => {
  if (Array.isArray(objValue)) {
    return objValue.concat(srcValue)
  }
})

module.exports = merge(config, custom)
