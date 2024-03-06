/**
 * The router.
 * - Initializes app state according to start URL.
 * - Adapts app state when URL changes.
 */

import Vue from 'vue'
import VueRouter from 'vue-router'
import { MessageBox } from 'element-ui'
import Notebook from './components/dmx-notebook-main'
import store from './store/client'
import dm5 from 'dmx-api'

Vue.use(VueRouter)

const router = new VueRouter({
  routes: [
    {
      path: '/',
      name: 'default',
      component: Notebook
    },
    {
      path: '/topic/:topicId',
      name: 'topic',
      component: Notebook
    },
    {
      path: '/assoc/:assocId',
      name: 'assoc',
      component: Notebook
    },
    {
      path: '/topic/:topicId/:detail',
      name: 'topicDetail',
      component: Notebook
    },
    {
      path: '/assoc/:assocId/:detail',
      name: 'assocDetail',
      component: Notebook
    }
  ]
})

export default router

store.registerModule('routerModule', {

  state: {
    router
  },

  actions: {

    initialNavigation () {
      initialNavigation(router.currentRoute)
    },

    callRoute (_, location) {
      // console.log('callRoute', location)
      router.push(location)
    },

    stripSelectionFromRoute () {
      router.push({
        name: 'default'
      })
    }
  }
})


// TODO: why does the watcher kick in when an initial URL is present?
// Since when is it this way?
function registerRouteWatcher () {
  store.watch(
    state => state.routerModule.router.currentRoute,
    (to, from) => {
      // console.log('### Route watcher', to, from)
      navigate(to, from)
    }
  )
}

// ### TODO: unify these 2 functions?

/**
 * Sets up initial app state according to start URL.
 */
function initialNavigation (route) {
  //
  registerRouteWatcher()
}

/**
 * Adapts app state when route changes.
 */
function navigate (to, from) {
  // 1) topic/assoc selection
  const topicId = id(to.params.topicId)
  const assocId = id(to.params.assocId)
  const oldTopicId = id(from.params.topicId)
  const oldAssocId = id(from.params.assocId)
  const topicChanged = topicId !== oldTopicId
  const assocChanged = assocId !== oldAssocId
  if (topicChanged && topicId !== undefined) {             // Note: 0 is a valid topic ID
    fetchTopic(topicId)
  }
  if (assocChanged && assocId) {
    fetchAssoc(assocId)
  }
  if ((topicChanged || assocChanged) && topicId === undefined && !assocId) {    // Note: 0 is a valid topic ID
    store.dispatch('emptyDisplay')
  }
  // 2) detail panel
  const detail = to.params.detail
  const oldDetail = from.params.detail
  if (detail != oldDetail && detail) {
    store.dispatch('selectDetail', detail)
  }
}

/**
 * @return  an ID (type Number) or undefined
 */
function objectId(route) {
  return id(route.params.assocId || route.params.topicId)     // Note: 0 is a valid topic ID; check assoc ID first
}

/**
 * Converts the given value into Number.
 *
 * @return  the number, or undefined if `undefined` or `null` is given.
 *          Never returns `null`.
 *
 * @throws  if the given value is not one of Number/String/undefined/null.
 */
function id (v) {
  // Note: Number(undefined) is NaN, and NaN != NaN is true!
  // Note: dm5.utils.getCookie may return null, and Number(null) is 0 (and typeof null is 'object')
  if (typeof v === 'number') {
    return v
  } else if (typeof v === 'string') {
    return Number(v)
  } else if (v !== undefined && v !== null) {
    throw Error(`id() expects one of [number|string|undefined|null], got ${v}`)
  }
}
