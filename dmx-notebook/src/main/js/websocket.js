import DM5WebSocket from 'dm5-websocket'
import store from './store/client'

/* eslint no-new: 0 */

const MESSAGES = [
  'processDirectives',
  'newTopicType',     // TODO: refactor message processing from actions into a Vue event bus.
  'newAssocType',     // The app developer should not care about the 3 "new" messages as they
  'newRoleType'       // are handled internally by the "dm5" library (see type-cache.js).
]

new DM5WebSocket('systems.dmx.webclient', message => {
  if (MESSAGES.includes(message.type)) {
    store.dispatch('_' + message.type, message.args)
  }
})
