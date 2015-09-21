from django.conf.urls import url

from . import views

urlpatterns = [
    url(r'^upload/$', views.upload_view, name='upload'),
    url(r'^login/$', views.login_view, name='api-login'),
]
