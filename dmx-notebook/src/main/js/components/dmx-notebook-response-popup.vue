<template>
  <transition name="el-fade-in">
    <div class="popup" v-show="visible">
      <img v-if="status === 1" :src="vec_success" alt="Saving successful" />
      <img v-else-if="status === -1" :src="vec_failure" alt="Saving failed" />
      <img v-else class="loading" :src="vec_loading" alt="Waiting for response" />
    </div>
  </transition>
</template>

<script>
  import successSVG from '../../resources/note-success.svg';
  import failureSVG from '../../resources/note-failure.svg';
  import loadingSVG from '../../resources/note-loading.svg';

  export default { 
    data () {
        return {
          vec_success: successSVG,
          vec_failure: failureSVG, 
          vec_loading: loadingSVG,
        };
    },

    computed: {
      status () {
        return this.$store.state.noteStatus;
      },
      visible () {
        return this.$store.state.popupVisible;
      },
    },
  }
</script>

<style>
.popup {
  height: 100vh;
  width: 100vw;
  top: 0;
  left: 0;
  z-index: 100;
  position: absolute;
  display: flex;
  justify-content: center;
  align-items: center;
}

.popup img {
  width: 35vw;
}

.loading {
  animation-name: loading-cycle;
  animation-duration: 1s;
  animation-timing-function: linear;
  animation-iteration-count: infinite; 
}

@keyframes loading-cycle {
    from {transform: rotate(0deg);}
    to {transform: rotate(360deg);}
}
</style>
