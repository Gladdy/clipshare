from django.contrib.auth.models import BaseUserManager
from django.utils import timezone


class UserManager(BaseUserManager):
    def _create_user(self, email, password, is_staff, is_superuser):
        """
        Creates and saves a User with the given username, email and password.
        """
        user = self.model(email=self.normalize_email(email),
                          is_staff=is_staff,
                          is_active=True,
                          is_superuser=is_superuser,
                          date_joined=timezone.now())
        user.set_password(password)
        user.save(using=self._db)
        return user

    def create_user(self, email, password=None):
        return self._create_user(email, password, False, False)

    def create_superuser(self, email, password):
        return self._create_user(email, password, True, True)