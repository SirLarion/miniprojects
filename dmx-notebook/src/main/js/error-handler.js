import { Notification } from 'element-ui'

export default function onHttpError (error) {
  const response = error.response
  Notification.error({
    title: 'Connecting to DMX server failed',
    duration: 2500
  })
}
