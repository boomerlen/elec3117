from django.urls import path 

from . import views 

urlpatterns = [
    path("", views.index, name="index"),
    path("data_entry/", views.handle_data, name="handle_data"),
    path("<str:device_id>/", views.soil_moisture_by_id, name="soil_moisture"),
]