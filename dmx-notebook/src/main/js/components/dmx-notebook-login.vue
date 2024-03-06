<template>
  <div class="note-login">
    <img :src="logo" class="title" alt="dmx-notebook" />
    <div>
      <el-button class="login-button" @click="openLogin">Login</el-button>
    </div>
  </div>
</template>

<script>

//TODO: working inline source for image
import logo from '../../resources/dmx-notebook-logo.svg';

export default {
  data () {
    return {
      logo: logo,
    };
  },

  methods: {
    openLogin() {
      this.$store.dispatch('openLoginDialog');
    },
  },
};
</script>

<style>

.note-login .title {
  width: 30vw;
  margin: 0;
}

.note-login {
  min-height: 100vh;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  background-color: var(--bg-header);
}

.login-button {
  margin-top: 3vh !important;
  border-radius: 1vw !important;
  color: var(--bg-header) !important;
  background-color: var(--bg-main) !important;
}

</style>
