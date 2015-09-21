from django.conf.urls import include, url

from emailauth.urls import urlpatterns as emailurls
from api.urls import urlpatterns as apiurls
from clipshare import views

urlpatterns = [
    #Index
    url(r'^$', views.index, name='index'),

    #Static pages
    url(r'^terms/$', views.terms, name='terms'),

    #Dynamic pages
    url(r'', include(emailurls)),

    #API endpoints
    url(r'^api/', include(apiurls)),


    #url(r'^admin/', include(admin.site.urls)),
]
