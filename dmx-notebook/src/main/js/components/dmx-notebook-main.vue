<template>
  <div class="dmx-notebook">
    <div v-if="username" class="main">
      <div class="header">
        <img :src="logoFlat" class="title" alt="dmx notebook" />
        <dmx-notebook-menu></dmx-notebook-menu>
      </div>
      <dmx-notebook-note></dmx-notebook-note>
      <response-popup></response-popup>
    </div>
    <dmx-notebook-login v-else></dmx-notebook-login>
    <dm5-login-dialog :visible="loginVisible" @logged-in="loggedIn" @close="closeLogin"></dm5-login-dialog>
  </div>
</template>

<script>
import dm5 from 'dmx-api';

//TODO: working inline source for image
import logoFlat from '../../resources/dmx-notebook-logo-flat.svg';

export default {
  data () {
    return {
      logoFlat: logoFlat,
    }
  },

  computed: {
    username () {
      return this.$store.state.login.username;
    },
    loginVisible () {
      return this.$store.state.login.visible;
    },
  },

  methods: {
    loggedIn (username) {
      this.$store.dispatch('loggedIn', username);
      this.$store.dispatch('checkInitConditions', username);
    },
    closeLogin () {
      this.$store.dispatch('closeLoginDialog');
    },
  },
  
  components: { 
    'dmx-notebook-note':     require('./dmx-notebook-note').default,
    'dmx-notebook-menu':     require('./dmx-notebook-menu').default,
    'dmx-notebook-login':    require('./dmx-notebook-login').default,
    'response-popup':        require('./dmx-notebook-response-popup').default,
    'dm5-login-dialog':      require('dm5-login-dialog').default,
  },
}
</script>

<style>

.dmx-notebook {
  min-height: 100%;
  flex-grow: 1;
  background-color: var(--background-color);
}

.title {
  font-size: 3vh;
  font-weight: 400;
  color: var(--fg-light);
  margin-left: 3vh;
}
.header .title {
  width: 45vw;
}

.main { /* The 'body' of dmx-notebook */
  min-height: 100vh; /* Header = 8vh */
  background-color: var(--bg-main);
}

.header {
  min-height: 8vh;
  display: flex;
  justify-content: space-between;
  align-items: center;
  background-color: var(--bg-header);
}

</style>
