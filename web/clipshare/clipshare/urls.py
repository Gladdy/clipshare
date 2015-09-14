from django.conf.urls import include, url
from django.contrib import admin

from . import views

urlpatterns = [
    url(r'^$', views.index, name='index'),
    url(r'^register/$', views.register_view, name='register'),

    url(r'^login/$', views.login_view, name='login'),
    url(r'^login-api/$', views.apilogin_view, name='login-api'),
    url(r'^logout/$', views.logout_view, name='logout'),
    url(r'^terms/$', views.terms, name='terms'),
    url(r'^upload/$', views.upload_view, name='upload'),
    url(r'^admin/', include(admin.site.urls)),
]
