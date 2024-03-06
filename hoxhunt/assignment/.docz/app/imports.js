export const imports = {
  'src/app/components/Typography/Typography.mdx': () =>
    import(
      /* webpackPrefetch: true, webpackChunkName: "src-app-components-typography-typography" */ 'src/app/components/Typography/Typography.mdx'
    ),
  'src/app/components/card/HeroCard/HeroCard.mdx': () =>
    import(
      /* webpackPrefetch: true, webpackChunkName: "src-app-components-card-hero-card-hero-card" */ 'src/app/components/card/HeroCard/HeroCard.mdx'
    ),
}
